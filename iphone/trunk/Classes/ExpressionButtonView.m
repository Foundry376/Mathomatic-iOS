//
//  ExpressionButtonView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/26/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "ExpressionButtonView.h"


@implementation ExpressionButtonView

@synthesize expression;
@synthesize expressionImageView;

- (id)initWithExpression:(MathomaticExpression*)e
{
    if (self = [self initWithFrame: CGRectZero])
    {
        [self setExpression: e];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        expressionImageView = [[UIImageView alloc] initWithFrame: frame];
        [self addSubview: expressionImageView];
    }
    return self;
}

- (void)setExpression:(MathomaticExpression*)e
{
    [expression release];
    expression = [e retain];
    [expressionImageView setImage: [expression equationImage]];
    [self fitToFrame: [self frame]];
}

- (void)dealloc 
{
    [expression release];
    [expressionImageView release];
    [super dealloc];
}


- (void)fitToFrame:(CGRect)frame
{
    CGSize max = CGSizeMake(310, 110);
    CGSize content = [[expressionImageView image] size];
    CGSize padding = CGSizeMake(8,4);
    
    max.height = fmin(max.height, content.height + padding.height * 2);
    
    float scaleFactor = fminf(1, (max.height - padding.height * 2) / content.height);
    scaleFactor = fminf(scaleFactor, (max.width - padding.width * 2) / content.width);
        
    CGRect content_fit = CGRectMake(padding.width, 
                                    padding.height,
                                    content.width * scaleFactor, 
                                    content.height * scaleFactor);
    
    showTip = NO;
    if (scaleFactor < 0.65){
        showTip = YES;
        [self setFrame: CGRectMake(self.frame.origin.x, self.frame.origin.y, content.width*scaleFactor + padding.width * 2, content.height*scaleFactor + padding.height * 2 + 18)];
    }else
        [self setFrame: CGRectMake(self.frame.origin.x, self.frame.origin.y, content.width*scaleFactor + padding.width * 2, content.height*scaleFactor + padding.height * 2)];
    [expressionImageView setFrame: content_fit];
}

- (void)drawRect:(CGRect)rect
{
    CGSize size = [self frame].size;
    
    float scale = size.height / 110.0;
    
    CGContextRef c = UIGraphicsGetCurrentContext();
    CGContextSaveGState(c);
    CGContextTranslateCTM(c, 0, size.height);
    CGContextScaleCTM(c, 1, -1);
    
    CGImageRef left;
    CGImageRef back;
    CGImageRef right;
    
    if ([self state] == UIControlStateNormal){
        left = [[UIImage imageNamed: @"equation_normal_left.png"] CGImage];
        back = [[UIImage imageNamed: @"equation_normal.png"] CGImage];
        right = [[UIImage imageNamed: @"equation_normal_right.png"] CGImage];
    } else{
        left = [[UIImage imageNamed: @"equation_highlight_left.png"] CGImage];
        back = [[UIImage imageNamed: @"equation_highlight.png"] CGImage];
        right = [[UIImage imageNamed: @"equation_highlight_right.png"] CGImage];
    }
    
    if (showTip){
        CGContextDrawImage(c, CGRectMake(0, 0, 320, 18), [[UIImage imageNamed:@"viewLarger.png"] CGImage]);
        CGContextTranslateCTM(c, 0, 18);
        size.height -= 18;
    }
    float a = floor(25 * scale);
    
    CGContextDrawImage(c, CGRectMake(0, 0, a, size.height), left);
    CGContextDrawImage(c, CGRectMake(a, 0, size.width - 2*a, size.height), back);
    CGContextDrawImage(c, CGRectMake(size.width - a, 0, a, size.height), right);

    CGContextRestoreGState(c);
}



- (BOOL)beginTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event
{
    [self setNeedsDisplay];
    return [super beginTrackingWithTouch: touch withEvent: event];
}

- (void)cancelTrackingWithEvent:(UIEvent *)event
{
    [self setNeedsDisplay];
    return [super cancelTrackingWithEvent: event];
}

- (void)endTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event
{
    [self setNeedsDisplay];
    return [super endTrackingWithTouch: touch withEvent: event];
}

@end
